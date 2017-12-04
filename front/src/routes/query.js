import React from 'react';
import BasePage from '../components/BasePage'
import { connect } from 'react-redux';
import { Row, Col, Card, Icon } from 'antd';

class Page extends BasePage {
    componentDidMount() {
        this.props.dispatch({
            type: 'repository/get',
            payload: { keyword: this.props.match.params.keyword }
        });
    }
    page() {
        console.log(this.props.repositories)
        return <div>
            <Row type="flex" gutter={ 32 }>
                {
                    this.props.repositories
                        .map(repository => <Col span={ 4 } >
                            <Card
                                title={ repository.name }
                                extra={ <Icon type="right-circle-o" /> }
                            >
                                description
                            </Card>
                        </Col>)
                }
            </Row>
        </div>;
    }
}

export default connect(
    ({ repository }) => ({ repositories: repository.list })
)(Page);