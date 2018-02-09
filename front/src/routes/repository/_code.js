import React, { Component } from 'react';
import { Card } from 'antd';
import ReactMarkdown from 'react-markdown';


class CodeTemplate extends Component {
    state = { initialized: false }

    componentWillMount () {
        this.props.dispatch({
            type: 'blob/getTextBlob',
            payload: {
                repositoriesName: this.props.repositoriesName,
                repositoryName: this.props.repositoryName,
                branchName: this.props.branchName,
                path: this.props.path
            },
            after: () => this.setState({ initialized: true })
        });
    }

    renderWaiting () {
        return <span>waiting...</span>;
    }

    renderContent () {
        return <Card
            title={ this.props.name }>
            <ReactMarkdown source={ this.props.content } />
        </Card>
    }

    render() {
        return this.state.initialized ? this.renderContent () : this.renderWaiting ();
    }
}

export default CodeTemplate;